FLAGS = -std=c99 -Wall -Werror -Wno-unused -g -rdynamic -lm
INCLUDE = -I code/ -I extern/

prebuild:
	mkdir -p build
	gcc code/build/main.c -o build/build $(FLAGS) $(INCLUDE)

program: prebuild
	build/build

dynamic: prebuild
	build/build dynamic

run: program
	(cd bin && ./jam)

web: prebuild
	build/build web

webrun:
	(source extern/emsdk/emsdk_env.sh && \
	emrun bin/main.html --verbose --system_info)

webwin: clean web
	cp -r bin/ ../wduck/
	(cd ../wduck && git add . && git commit -m "update" && git push origin master)

webrelease:
	build/build web release
	mv bin/index* release/DuckMarch/
	zip -r release/DuckMarch.zip release/DuckMarch

clean:
	rm bin/ -r
	rm build/ -r
