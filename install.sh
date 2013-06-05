function clean()
{
	version=
	atom=
	qib=
	exit
}

function mm()
{
echo ATOM=$1 > remoteServer/Makefile
cat >> remoteServer/Makefile << "EOF"
PROJECT = .
HOST	=134.109.151.142#10.42.0.55#169.254.130.18
PORT	=32768

.PHONY: clean aatom catom matom edit

aatom:
	qitoolchain create cross-atom $(ATOM)
	qibuild configure --release -c cross-atom $(PROJECT)
	qibuild make --release -v -c cross-atom $(PROJECT)

catom:
	qibuild configure --release -v -c cross-atom $(PROJECT)
matom:
	qibuild make --release -v -c cross-atom $(PROJECT)

clean:
	qibuild clean -f

scp:
	scp ./build-cross-atom-release/sdk/bin/remoteServer nao@$(HOST):rServer
	scp -r ./build-cross-atom-release/sdk/lib/ nao@$(HOST):RMlib

telnet:
	telnet $(HOST) $(PORT)

edit:
	gedit *.cpp *.h &
EOF
}

version=nao-atom-cross-toolchain-1.12.3
echo "[INFO]Suche $version/toolchain.xml..."
atom=`find ~ | grep $version/toolchain.xml | head -n1`
qib=`which qibuild`

if [ -n `echo $atom | grep $version/toolchain.xml` ]
then
	echo -e ['\e[32mOK\e[0m']$atom
else
	echo -e ['\e[31mFAIL\e[0m']$version/toolchain.xml konnte nicht gefunden werden
	clean
fi

sleep 1
echo "[INFO]Suche qibuild in \$PATH..."
if [ -n $qib ]
then
	echo -e ['\e[32mOK\e[0m']$qib
else
	echo -e ['\e[31mFAIL\e[0m']qibuild konnte nicht gefunden werden, bitte installieren Sie es nach.
	clean
fi

sleep 1
echo "[INFO] create remoteServer/Makefile..."
mm $atom
sleep 1
echo "[INFO]>qibuild init -f"
qibuild init -f
echo "[INFO]>cd remoteServer"
cd remoteServer
echo "[INFO]>make aatom"
sleep 2
make clean
make aatom
echo "[INFO]>cd .."
cd ..



clean

