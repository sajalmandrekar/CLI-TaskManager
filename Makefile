app: task.o
	g++ task.o -o task.out

task.o: task.cpp
	g++ -c -std=c++11 -Wall task.cpp -o task.o

test: app
	npm run test

clean:
	rm -f task.o task.out

install:
	npm install

remove:
	rm -f task.txt complete.txt

git:
	git add .
	git commit -m "default commit"

push:
	git push mtask master:main
