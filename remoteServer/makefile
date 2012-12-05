PROJECT = .
SDK	=/home/jierr/development/naoqi-sdk-1.12.3-linux64
.PHONY: clean aatom  catom matom  host 

host:
	qitoolchain create stdtool $(SDK)/toolchain.xml --default
	qibuild configure  -c stdtool  $(PROJECT)
	qibuild make $(PROJECT)

aatom:
	qitoolchain create cross-atom $(SDK)/toolchain/nao-atom-cross-toolchain-1.12.3/toolchain.xml
	qibuild configure --release -c cross-atom $(PROJECT)
	qibuild make --release -v -c cross-atom $(PROJECT)

catom:
	qibuild configure --release -v -c cross-atom $(PROJECT)
matom:
	qibuild make --release -v -c cross-atom $(PROJECT)

clean:
	qibuild clean -f
