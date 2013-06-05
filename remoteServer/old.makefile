PROJECT = .
SDK	=/home/jierr/development/naoqi-sdk-1.12.3-linux64
HOST	=134.109.151.142#10.42.0.55#169.254.130.18
PORT	=32768
#ATOM	=$(shell  find -name nao-atom-cross-toolchain-1.12.3 | head -n1)

.PHONY: clean aatom  catom matom host edit

host:
	qitoolchain create stdtool $(SDK)/toolchain.xml --default
	qibuild configure  -c stdtool  $(PROJECT)
	qibuild make $(PROJECT)

aatom:
	qitoolchain create cross-atom $(SDK)/toolchain/nao-atom-cross-toolchain-1.12.3/toolchain.xml
	        #qitoolchain create cross-atom $(SDK)/toolchain/linux32-nao-atom-cross-toolchain-1.14/toolchain.xml
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
