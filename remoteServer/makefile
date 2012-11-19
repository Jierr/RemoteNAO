PROJECT = .
SDK	=/home/jierr/development/naoqi-sdk-1.12.3-linux32
.PHONY: clean atom geode host

host:
	qitoolchain create stdtool $(SDK)/toolchain.xml --default
	qibuild configure --release $(PROJECT)
	qibuild make $(PROJECT)

atom:
	qitoolchain create cross-atom $(SDK)/toolchain/nao-atom-cross-toolchain-1.12.3/toolchain.xml
	qibuild configure --release -c cross-atom $(PROJECT)
	qibuild make --release -c cross-atom $(PROJECT)
clean:
	qibuild clean
