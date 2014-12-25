1. 进入工程目录
	qmake
	make
2. 将3rdparty中对应平台的库文件拷贝至bin目录

3. 讲deploy中全部文件拷贝至bin目录




开启FTP:
	tcpsvd 0 21 ftpd -w /&
开启qitbin目录写权限:
	mount -o rw,remount /









#####################################################
目录结构
	3rdparty/: 外部依赖头文件、库，按架构划分(linux/、win32/、mac/、...)。 

	bin/: 目标文件生成目录(exe、dll、...)  分发目录，最终发布的可执行程序和各种运行支持文件存放在此目录，打包此目录即可完成项目分发。

	build/: 项目编译目录，各种编译的临时文件和最终的目标文件皆存于此，分为debug/和release/子目录。

	deploy/: 项目部署文件、配置文件等。

	doc/: 保存项目各种文档。

	res/: 资源目录。 (+)

	samples/: 样例程序目录。

	script/: 脚本目录。(+)

	src/: 源代码

	tools/: 项目支撑工具目录 (+)

	Devlog/: 开发日志记录 (+)

	Makefile:项目构建配置文件

	README:项目的总体说明文件
