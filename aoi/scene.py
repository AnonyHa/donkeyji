# -*- coding:utf-8 -*-

############################################## 
class Entity:
	def __init__(self):
		self.id = -1
		self.x = -1
		self.y = -1
		self.ownerGrid = -1 #所在的场景grid
		self.ownerScene = -1 #所在的场景
		self.visibleEntitys = {}# 需要进行同步的entity

	# ------------
	# 基本属性操作
	# ------------
	def setId(self, id):
		self.id = id

	def setXY(self, x, y):
		self.x = x
		self.y = y

	def getXY(self):
		return self.x, self.y

	# ------------
	# 可视列表操作
	# ------------
	def addVisibleEntity(self, entity):
		self.visibleEntitys[entity.getId()] = entity

	def removeVisibleEntity(self, entity):
		self.visibleEntitys[entity.getId()] = None

	# --------------------
	# 对外的场景变换的接口
	# --------------------
	def enterScene(self, scene, x, y):
		scene.onEntityEnter(self, x, y)

	def leaveScene(self):
		self.ownerScene.onLeaveScene(self)

	# -----------------------
	# 进入新场景时由scene回调
	# -----------------------
	def onEnterScene(self, scene, grid, x, y):
		oldGrid = self.ownerGrid
		newGrid = grid
		oldScene = -1
		newScene = scene

		self.ownerScene = scene
		self.ownerGrid = grid
		self.x = x
		self.y = y

		self.__updateAOI(oldGrid, newGrid, oldScene, newScene)
		self.broadcastSelf2Neighbors()

	# -------------------------
	# 离开当前场景，由scene回调
	# -------------------------
	def onLeaveScene(self):
		oldGrid = self.ownerGrid
		newGrid = grid
		oldScene = self.ownerScene
		newScene = -1

		# 置空
		self.ownerScene = -1
		self.ownerGrid = -1
		self.x = -1
		self.y = -1

		self.__updateAOI(oldGrid, newGrid, oldScene, newScene)
		#self.broadcastSelf2Neighbors() 无需广播

	# ----------------------
	# 在当前场景的走动的接口
	# ----------------------
	def moveTo(self, x, y):
		oldGrid = self.ownerGrid
		newGrid = self.ownerScene.getGrid(x, y)

		oldScene = self.ownerScene
		newScene = self.ownerScene

		self.x = x
		self.y = y
		self.ownerGrid = newGrid

		if oldGrid != newGrid:# grid内的移动，不处理AOI变化
			self.__updateAOI(oldGrid, newGrid, oldScene, newScene)

		# 只要移动，始终broadcast给邻居
		self.broadcastSelf2Neighbors()

	# ------------------
	# this进入other的AOI
	# ------------------
	def onEnterOtherAOI(self, otherEntity):
		self.sendSelfEnter2Other(otherEntity)

	# ------------------
	# this离开other的AOI
	# ------------------
	def onLeaveOtherAOI(self, otherEntity):
		self.sendSelfLeave2Other(otherEntity)

	# -----------------------
	# 这下面2个是消息发送接口
	# -----------------------
	def sendSelfEnter2Other(self, otherEntity):
		pass
	def sendSelfLeave2Other(self, otherEntity):
		pass

	# ------------------------------
	# 内部接口，处理了位置信息的广播
	# ------------------------------
	def __updateAOI(self, oldGrid, newGrid, oldScene, newScene):
		newVisibleGrids, newHideGrids = self.ownerScene.getVisibleAndHideGrids(self, oldGrid, newGrid, oldScene, newScene)

		for grid in newVisibleGrids:
			allGridEntitys = grid.getEntitys()
			for entityId in allGridEntitys:
				if allGridEntitys[entityId] != self:# 包含了自身所在的grid
					self.addVisibleEntity(allGridEntitys[entityId]) # other entity加入this的可视列表
					allGridEntitys[entityId].addVisibleEntity(self) # this加入other entity的可视列表
					allGridEntitys[entityId].onEnterOtherAOI(self) # other entity进入this的AOI后的回调
					#self.onEnterOtherAOI(allGridEntitys[entityId]) # 此处不调用，而由broadcastSelf2Neighbors来集中同步自身信息给邻居

		for grid in newHideGrids:
			allGridEntitys = grid.getEntitys()
			for entityId in allGridEntitys:
				if allGridEntitys[entityId] != self:# 包含了自身所在的grid
					allGridEntitys[entityId].removeVisibleEntity(self)
					self.removeVisibleEntity(allGridEntitys[entityId])
					allGridEntitys[entityId].onLeaveOtherAOI(self)# other entity离开this的AOI后的回调
					self.onLeaveOtherAOI(allGridEntitys[entityId])

############################################## 
class Grid:
	def __init__(self, width, height, i, j, scene):
		self.ownerScene = scene
		self.width = width 
		self.height = height
		self.i = i
		self.j = j
		# 这2个属性会随时改变
		self.entitys = {} # id为key

	def getIJ(self):
		return self.i, self.j

	# -------------
	# entitys的接口
	# -------------
	def getEntitys(self):
		return entitys

	def addEntity(self, entity):
		self.entitys[entity.getId()] = entity

	def removeEntity(self, entity):
		self.entitys[entity.getId()] = None


############################################## 
class Scene:
	def __init__(self):
		self.proto = -1
		self.id = -1 
		self.width = -1
		self.height = -1
		self.gridWidth = -1
		self.gridHeight = -1
		#self.nI = self.height / self.gridHeight#行
		#self.nJ = self.width / self.gridWidth#列
		# 这2个属性会随时改变	
		self.entitys = {}
		#self.__initGrids(self)

	# 从文件读取scene的配置来生成scene对象
	def loadScene(self, confFile):
		# 初始化proto, id, width, height, gridHeight, gridWidth, nI, nJ, entitys, grids
		pass

	def __initGrids(self):
		self.grids = []
		for i in range(self.nI):#行
			self.grids[i] = []
			for j in range(self.nJ):#列
				self.grids[i][j] = Grid(self.gridWidth, self.gridHeight, i, j, self)

	def __xyToij(self, x, y):
		i = y / self.gridHeight
		j = x / self.gridWidth
		return i, j

	def getEntitys(self):
		return self.entitys

	def getGrid(self, x, y):
		i, j = self.__xyToij(x, y)
		return self.grids[i][j]

	# ------------------
	# 进入场景时指定x, y 
	# ------------------
	def onEntityEnter(entity, x, y):
		i, j = self.__xyToij(x, y)
		self.grids[i][j].addEntity(entity)
		self.entitys[entity.getId()] = entity#都是以id为key保存entity
		entity.onEnterScene(self, self.grids[i][j], x, y)# 回调

	# --------------------
	# 离开场景时不指定x, y
	# --------------------
	def onEntityLeave(entity):
		i, j = self.__xyToij(entity.getXY())
		self.grids[i][j].removeEntity(entity)
		self.entitys[entity.getId()] = None
		entity.onLeaveScene() # 回调

	# ---------------------------------------------------------
	# 从oldGrid到newGrid时，产生的newVisibleGrids, newHideGrids
	# 考虑oldGrid与newGrid在不同场景时的情况
	# 当entity从其他场景进入本场景，
	# ---------------------------------------------------------
	def getVisibleAndHideGrids(self, entity, oldGrid, newGrid, oldScene, newScene):
		newVisibleGrids = []
		newHideGrids = []
		if oldScene != newScene:# 切换场景
			if oldScene == -1:# 进入新场景
				newVisibleGrids = self.__getNeighborGrid(newGrid)
				return newVisibleGrids, newHideGrids
			if newScene == -1:# 离开当前场景
				newHideGrids = self.__getNeighborGrid(oldGrid)
				return newVisibleGrids, newHideGrids
		else:# 同一场景内
			oldNeighborGrids = self.__getNeighborGrid(oldGrid)
			newNeighborGrids = self.__getNeighborGrid(newGrid)
			newVisibleGrids = self.__getNewVisibleGrids(oldNeighborGrids, newNeighborGrids)
			newHideGrids = self.__getNewHideGrids(oldNeighborGrids, newNeighborGrids)
			return newVisibleGrids, newHideGrids


	# ------------------------------------------
	# 包括grid自身以及周围的8个grid，一共9个grid
	# ------------------------------------------
	def __getNeighborGrid(self, grid):
		pass

	# ----------------------
	# new中有而old中无的grid
	# ----------------------
	def __getNewVisibleGrids(self, oldNeighborGrids, newNeighborGrids):
		pass

	# ----------------------
	# old中有而new中无的grid
	# ----------------------
	def __getNewHideGrids(self, oldNeighborGrids, newNeighborGrids):
		pass

############################################## 
class SceneMgr:
	def __init__(self):
		self.scenes = {}
		self.__loadAllScene()

	def __loadAllScene(self):
		confFile = self.__getConfFile()
		scene = Scene()
		scene.loadScene(confFile)
		self.scenes[scene.getProto()] = scene

	def getSceneByProto(self, proto):
		return self.scenes[proto]
############################################## 
def main():
	sceneMgr = SceneMgr()
	user1 = Entity()
	user2 = Entity()
	sceneObj = sceneMgr.getSceneByProto(2001)
	user1.enterScene(sceneObj, 10, 20)
	user2.enterScene(sceneObj, 10, 20)
	user1.moveTo(21, 31)
	user2.moveTo(41, 21)

############################################## 
if __name__ == '__main__':
	main()
