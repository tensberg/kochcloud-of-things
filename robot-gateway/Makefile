proto_path = ../gateway-robot/proto

init:
	pip install -r requirements.txt

clean:
	rm -rf robobuf

# requires apt install protobuf-compiler
proto:
	protoc --proto_path=robobuf=$(proto_path) --python_out=. $(proto_path)/*.proto

test:
	py.test

.PHONY: init clean test
