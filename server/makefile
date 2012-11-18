PROJECT = .
SDK	=/home/jierr/development/naoqi-sdk-1.12.3-linux32
.PHONY: clean atom geode host

host:
	qibuild init
	qitoolchain create stdtool $(SDK)/toolchain.xml --default
	qibuild configure $(PROJECT)
	qibuild make $(PROJECT)

atom:
	qibuild init
	qitoolchain create cross-atom $(SDK)/toolchain/nao-atom-cross-toolchain-1.12.3/toolchain.xml
	qibuild configure -c cross-atom $(PROJECT)
	qibuild make -c cross-atom $(PROJECT)

geode:
	qibuild init
	qitoolchain create cross-geode $(SDK)/toolchain/nao-geode-cross-toolchain-1.12.3/toolchain.xml
	qibuild configure -c cross-geode $(PROJECT)
	qibuild make -c cross-geode $(PROJECT)

clean:
	qibuild clean
