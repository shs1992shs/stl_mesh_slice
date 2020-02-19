# stl_mesh_slice
slice stl mesh for 3D printing
基于C++标准STL库建立STL网格模型之间的拓扑关系
确定每层切片平面的相交三角面片，建立邻接拓扑的网格面片环
基于网格面片环确定有序排列的公共邻接边
再通过公共邻接边求取有序的截面交点
基于OpenGL实现了STL模型、单层相交三角面片，切片轮廓的可视化
