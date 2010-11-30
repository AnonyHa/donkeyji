<?php
//===========================================
// ����Memcache-Based Session Class
// ���ܣ�����Memcache�洢�� Session ������
// ����: heiyeluren
// ����: http://blog.csdn.net/heiyeshuwu
// ʱ��: 2006-12-23
//===========================================


/**
 * ����: FileSession Class
 * ����: ����ʵ�ֻ���Memcache�洢�� Session ����
 * ����: ��������ʵ��Session�Ĺ���, ��������ͨ�����ÿͻ��˵�Cookie������SessionID,
 *         Ȼ����û������ݱ����ڷ�������,���ͨ��Cookie�е�Session Id��ȷ��һ�������Ƿ����û���, 
 *         Ȼ�������Ӧ�����ݲ���, Ŀǰ��ȱ����û�������ռ�����
 *
 *        ����ʽ�ʺ�Memcache�ڴ淽ʽ�洢Session���ݵķ�ʽ��ͬʱ��������ֲ�ʽ��Memcache��������
 *        �ܹ������൱�໺�����ݣ������ʺ��û����Ƚ϶ಢ���Ƚϴ�����
 * ע��: �������Ҫ��PHP��װ��Memcache��չ, ��ȡMemcache��չ�����: http://pecl.php.net
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
     * ���캯��
     *
     * @param bool $isInit - �Ƿ�ʵ���������ʱ������Session
     */
    function MemcacheSession($isInit = false){
        if ($isInit){
            $this->start();
        }
    }

    //-------------------------
    //   �ⲿ����
    //-------------------------
    
    /**
     * ����Session����
     *
     * @param int $expireTime - SessionʧЧʱ��,ȱʡ��0,��������رյ�ʱ��ʧЧ, ��ֵ��λ����
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
     * �ж�ĳ��Session�����Ƿ�ע��
     *
     * @param string $varName - 
     * @return bool ���ڷ���true, �����ڷ���false
     */
    function is_registered($varName){
        if (!isset($_SESSION[$varName])){
            return false;
        }
        return true;
    }    
        
    /**
     * ע��һ��Session����
     *
     * @param string $varName - ��Ҫע���Session�ı�����
     * @param mixed $varValue - ע���Session������ֵ
     * @return bool - �ñ������Ѿ����ڷ���false, ע��ɹ�����true
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
     * ����һ����ע���Session����
     *
     * @param string $varName - ��Ҫ���ٵ�Session������
     * @return bool ���ٳɹ�����true
     */
    function unregister($varName){
        unset($_SESSION[$varName]);
        $this->_saveSession();
        return true;
    }
    
    /**
     * ���������Ѿ�ע���Session����
     *
     * @return ���ٳɹ�����true
     */
    function destroy(){
        $_SESSION = array();
        $this->_saveSession();
        return true;    
    }
    
    /**
     * ��ȡһ����ע���Session����ֵ
     *
     * @param string $varName - Session����������
     * @return mixed - �����ڵı�������false, ���ڱ������ر���ֵ
     */
    function get($varName){
        if (!isset($_SESSION[$varName])){
            return false;
        }
        return $_SESSION[$varName];
    }    
    
    /**
     * ��ȡ����Session����
     *
     * @return array - ����������ע���Session����ֵ
     */
    function getAll(){
        return $_SESSION;
    }
    
    /**
     * ��ȡ��ǰ��Session ID
     *
     * @return string ��ȡ��SessionID
     */
    function getSid(){
        return $this->sessId;
    }

    /**
     * ��ȡMemcache��������Ϣ
     *
     * @return array Memcache����������Ϣ
     */
    function getMemConfig(){
        return $this->memConfig;
    }
    
    /**
     * ����Memcache��������Ϣ
     *
     * @param string $host - Memcache��������IP
     * @param int $port - Memcache�������Ķ˿�
     */
    function setMemConfig($host, $port){
        $this->memConfig = array('host'=>$host, 'port'=>$port);
    }    
    
    
    //-------------------------
    //   �ڲ��ӿ�
    //-------------------------
    
    /**
     * ����һ��Session ID
     *
     * @return string ����һ��32λ��Session ID
     */
    function _getId(){
        return md5(uniqid(microtime()));
    }
    
    /**
     * ��ȡһ��������Memcache��Session Key
     *
     * @param string $sessId - �Ƿ�ָ��Session ID
     * @return string ��ȡ����Session Key
     */
    function _getSessKey($sessId = ''){
        $sessKey = ($sessId == '') ? $this->sessKeyPrefix.$this->sessId : $this->sessKeyPrefix.$sessId;
        return $sessKey;
    }    
    /**
     * ��鱣��Session���ݵ�·���Ƿ����
     *
     * @return bool �ɹ�����true
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
     * ��ȡSession�ļ��е�����
     *
     * @param string $sessId - ��Ҫ��ȡSession���ݵ�SessionId
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
     * �ѵ�ǰ��Session���ݱ��浽Memcache
     *
     * @param string $sessId - Session ID
     * @return �ɹ�����true
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
     * ��ʾ��ʾ��Ϣ
     *
     * @param string $strMessage - ��Ҫ��ʾ����Ϣ����
     * @param bool $isFailed - �Ƿ���ʧ����Ϣ, ȱʡ��true
     */
    function _showMessage($strMessage, $isFailed = true){
        if ($isFailed){
            exit($strMessage);
        }
        echo $strMessage;
    }    
}
?>
