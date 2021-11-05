CWD := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
MKDIR := mkdir -p
DB_DIR := docker/postgres/data/

BUILD_DIR := bin
PATH := $(BUILD_DIR):$(PATH)

SOURCES := $(wildcard src/broker/*.go)
GOARCH := $(shell go env GOARCH)
GOOS := $(shell go env GOOS)
GO_TARGET_DIR := $(BUILD_DIR)/$(GOOS)_$(GOARCH)
BROKER_EXEC := $(GO_TARGET_DIR)/broker

.PHONY: all
all: $(BROKER_EXEC)
all: directories

$(BROKER_EXEC): $(SOURCES)
	go build -o $@ $?

.PHONY: format
format:
	go fmt

.PHONY: test
test: golint vet

.PHONY: golint
golint: $(SOURCES)
	golint $^

.PHONY: vet
vet: $(SOURCES)
	go vet $^

.PHONY: clean
clean:
	$(RM) $(BROKER_EXEC)

.PHONY: directories
directories: ${DB_DIR}

${DB_DIR}:
	${MKDIR} ${DB_DIR}

.PHONY: run-webserver
run-webserver: directories $(BROKER_EXEC)
	# Work-around: Docker creates files with wrong permissions. For proper solution, see:
	# https://stackoverflow.com/questions/29245216/write-in-shared-volumes-docker
	sudo chown $(USER):$(USER) -R ${DB_DIR}
	docker-compose -f docker/docker-compose.yml up --build

.PHONY: create-ssl-certificates
create-ssl-certificates:
	scripts/create_ssl_cert.sh secrets
