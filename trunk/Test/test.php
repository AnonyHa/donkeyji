<?php
//===========================================
// 程序：Memcache-Based Session Class
// 功能：基于Memcache存储的 Session 功能类
// 作者: heiyeluren
// 博客: http://blog.csdn.net/heiyeshuwu
// 时间: 2006-12-23
//===========================================


/**
 * 类名: FileSession Class
 * 功能: 自主实现基于Memcache存储的 Session 功能
 * 描述: 这个类就是实现Session的功能, 基本上是通过设置客户端的Cookie来保存SessionID,
 *         然后把用户的数据保存在服务器端,最后通过Cookie中的Session Id来确定一个数据是否是用户的, 
 *         然后进行相应的数据操作, 目前的缺点是没有垃圾收集功能
 *
 *        本方式适合Memcache内存方式存储Session数据的方式，同时如果构建分布式的Memcache服务器，
 *        能够保存相当多缓存数据，并且适合用户量比较多并发比较大的情况
 * 注意: 本类必须要求PHP安装了Memcache扩展, 获取Memcache扩展请访问: http://pecl.php.net
 */
class MemcacheSession
{
    var $sessId                = '';
    var $sessKeyPrefix         = 'sess_';
    var $sessExpireTime        = 86400;
    var $cookieName         = '__SessHandler';
    var $cookieExpireTime     = '';    
    var $memConfig             = array('host'=>'192.168.0.200', 'port'=>11211);
    var $memObject            = null;    
    
    
    /**
     * 构造函数
     *
     * @param bool $isInit - 是否实例化对象的时候启动Session
     */
    function MemcacheSession($isInit = false){
        if ($isInit){
            $this->start();
        }
    }

    //-------------------------
    //   外部方法
    //-------------------------
    
    /**
     * 启动Session操作
     *
     * @param int $expireTime - Session失效时间,缺省是0,当浏览器关闭的时候失效, 该值单位是秒
     */
    function start($expireTime = 0){
        $sessId = $_COOKIE[$this->cookieName];
        if (!$sessId){
            $this->sessId = $this->_getId();
            $this->cookieExpireTime = ($expireTime > 0) ? time() + $expireTime : 0;
            setcookie($this->cookieName, $this->sessId, $this->cookieExpireTime, "/", '');
            $this->_initMemcacheObj();
            $_SESSION = array();
            $this->_saveSession();
        } else {
            $this->sessId = $sessId;
            $_SESSION = $this->_getSession($sessId);
        }        
    }
    
    /**
     * 判断某个Session变量是否注册
     *
     * @param string $varName - 
     * @return bool 存在返回true, 不存在返回false
     */
    function is_registered($varName){
        if (!isset($_SESSION[$varName])){
            return false;
        }
        return true;
    }    
        
    /**
     * 注册一个Session变量
     *
     * @param string $varName - 需要注册成Session的变量名
     * @param mixed $varValue - 注册成Session变量的值
     * @return bool - 该变量名已经存在返回false, 注册成功返回true
     */
    function register($varName, $varValue){
        if (isset($_SESSION[$varName])){
            return false;
        }
        $_SESSION[$varName] = $varValue;
        $this->_saveSession();
        return true;
    }
    
    /**
     * 销毁一个已注册的Session变量
     *
     * @param string $varName - 需要销毁的Session变量名
     * @return bool 销毁成功返回true
     */
    function unregister($varName){
        unset($_SESSION[$varName]);
        $this->_saveSession();
        return true;
    }
    
    /**
     * 销毁所有已经注册的Session变量
     *
     * @return 销毁成功返回true
     */
    function destroy(){
        $_SESSION = array();
        $this->_saveSession();
        return true;    
    }
    
    /**
     * 获取一个已注册的Session变量值
     *
     * @param string $varName - Session变量的名称
     * @return mixed - 不存在的变量返回false, 存在变量返回变量值
     */
    function get($varName){
        if (!isset($_SESSION[$varName])){
            return false;
        }
        return $_SESSION[$varName];
    }    
    
    /**
     * 获取所有Session变量
     *
     * @return array - 返回所有已注册的Session变量值
     */
    function getAll(){
        return $_SESSION;
    }
    
    /**
     * 获取当前的Session ID
     *
     * @return string 获取的SessionID
     */
    function getSid(){
        return $this->sessId;
    }

    /**
     * 获取Memcache的配置信息
     *
     * @return array Memcache配置数组信息
     */
    function getMemConfig(){
        return $this->memConfig;
    }
    
    /**
     * 设置Memcache的配置信息
     *
     * @param string $host - Memcache服务器的IP
     * @param int $port - Memcache服务器的端口
     */
    function setMemConfig($host, $port){
        $this->memConfig = array('host'=>$host, 'port'=>$port);
    }    
    
    
    //-------------------------
    //   内部接口
    //-------------------------
    
    /**
     * 生成一个Session ID
     *
     * @return string 返回一个32位的Session ID
     */
    function _getId(){
        return md5(uniqid(microtime()));
    }
    
    /**
     * 获取一个保存在Memcache的Session Key
     *
     * @param string $sessId - 是否指定Session ID
     * @return string 获取到的Session Key
     */
    function _getSessKey($sessId = ''){
        $sessKey = ($sessId == '') ? $this->sessKeyPrefix.$this->sessId : $this->sessKeyPrefix.$sessId;
        return $sessKey;
    }    
    /**
     * 检查保存Session数据的路径是否存在
     *
     * @return bool 成功返回true
     */
    function _initMemcacheObj(){
        if (!class_exists('Memcache') || !function_exists('memcache_connect')){
            $this->_showMessage('Failed: Memcache extension not install, please from http://pecl.php.net download and install');
        }        
        if ($this->memObject && is_object($this->memObject)){
            return true;
        }
        $mem = new Memcache;
        if (!@$mem->connect($this->memConfig['host'], $this->memConfig['port'])){
            $this->_showMessage('Failed: Connect memcache host '. $this->memConfig['host'] .':'. $this->memConfig['port'] .' failed');
        }
        $this->memObject = $mem;
        return true;
    }
    
    /**
     * 获取Session文件中的数据
     *
     * @param string $sessId - 需要获取Session数据的SessionId
     * @return unknown
     */
    function _getSession($sessId = ''){
        $this->_initMemcacheObj();
        $sessKey = $this->_getSessKey($sessId);
        $sessData = $this->memObject->get($sessKey);
        if (!is_array($sessData) || empty($sessData)){
            $this->_showMessage('Failed: Session ID '. $sessKey .' session data not exists');
        }
        return $sessData;
    }
    
    /**
     * 把当前的Session数据保存到Memcache
     *
     * @param string $sessId - Session ID
     * @return 成功返回true
     */
    function _saveSession($sessId = ''){
        $this->_initMemcacheObj();
        $sessKey = $this->_getSessKey($sessId);
        if (empty($_SESSION)){
            $ret = @$this->memObject->set($sessKey, $_SESSION, false, $this->sessExpireTime);
        }else{
            $ret = @$this->memObject->replace($sessKey, $_SESSION, false, $this->sessExpireTime);
        }
        if (!$ret){
            $this->_showMessage('Failed: Save sessiont data failed, please check memcache server');
        }
        return true;
    }
    
    /**
     * 显示提示信息
     *
     * @param string $strMessage - 需要显示的信息内容
     * @param bool $isFailed - 是否是失败信息, 缺省是true
     */
    function _showMessage($strMessage, $isFailed = true){
        if ($isFailed){
            exit($strMessage);
        }
        echo $strMessage;
    }    
}
?>
