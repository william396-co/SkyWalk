
## Navigation导航需要实现三个接口
	1. findPath， 获取起始点到目标点的路径
	2. findRandomPoint， 获取随机的点 
	3. rayCast， 射线，碰撞检测
### RecastNavigation ( 3D场景建模 )
	Recast&Detour本身是个独立的程序库，可以直接在工程中使用
	RecastDemo用到了SDL库，SDL内置调用了OpenGL，编译需要DirectX
+ Recast将场景网格模型生成用于寻路的网格模型NavMesh
	- NavMesh生成   
		* CritterAI（基于RecatNavigation制作，NMGen生成导航数据）
		* Recast for Unity插件（收费）
		* unity自带的NavMesh agent（需要实现额外的代码）
	- NavMesh的生成过程，RecastDemo/Sample_SoloMesh.cpp::handleBuild()
		1. 光栅化
		2. 过滤可行走表面
		3. 分割可行走表面为多边形
		4. 跟踪并简化区域轮廓
		5. 通过区域轮廓创建多边形网格
		6. 创建细节网格
+ Detour利用Recast生成的网格模型NavMesh进行寻路
	- Detour主要包含两个数据结构
		1. dtNavMesh 网格数据结构
		2. dtNavMeshQuery 网格寻路算法
	- NavMesh文件读取  
		1. Tile集合  
			```
			struct NavMeshSetHeader
			{
				int version;
				int tileCount;
				dtNavMeshParams params;
			}
			```
		2. Tile  
			```
			struct NavMeshtTileHeader
			{
				dtTileRef tileRef;
				int dataSize;
			}
			```
	- Navigation接口实现
		* findPath，调用dtNavmeshQuery::findStraightPath()
		* findRandomPoint，调用dtNavmeshQuery::findRandomPointCircle()
		* rayCast()，调用dtNavMeshQuery::rayCast()
### TiledMap ( 2D场景建模 )
	Tmx文件是TiledMap生成的地图文件，它是一个xml格式的文件  
+ Tmx文件生成
	- Unity：[Tiled2Unity](http://www.seanba.com/tiled2unity)，Unity2017开始支持TiledMap生成。
	- Cocos2d：TiledMapEditor。
+ TiledMap寻路   
	- tmx文件读取，使用RapidXML读取TMX文件。
		* Object，对象信息结构体
		* TileSet，瓦片信息结构体
		* Layer，层信息结构体
		* Image，图层信息结构体
	- Navigation接口实现    
		* findPath，使用A*算法，实现AStarState  
		* findRandomPoint，（x = 随机半径 * cos(随机角度)，y = 随机半径 * sin(随机角度)）
		* rayCast，使用Bresenhanm算法画线 
