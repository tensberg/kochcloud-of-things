proto_path = ../gateway-robot/proto

init:
	pip install -r requirements.txt

clean:
	rm -r robobuf/*_pb2.py

# requires apt install protobuf-compiler
# this only works if you add "robobuf" to all the import statements in the .proto files
proto:
	protoc --proto_path=robobuf=$(proto_path) --python_out=. $(proto_path)/*.proto

test:
	py.test

.PHONY: init clean test
