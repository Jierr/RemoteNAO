PROJECT = .
SDK	=/home/jierr/development/naoqi-sdk-1.12.3-linux32
.PHONY: clean atom geode host

host:
	qitoolchain create stdtool $(SDK)/toolchain.xml --default
	qibuild configure --release -v  $(PROJECT)
	qibuild make -v $(PROJECT)

atom:
	qitoolchain create cross-atom $(SDK)/toolchain/nao-atom-cross-toolchain-1.12.3/toolchain.xml
	qibuild configure --release -v -c cross-atom $(PROJECT)
	qibuild make --release -v -c cross-atom $(PROJECT)
clean:
	qibuild clean
