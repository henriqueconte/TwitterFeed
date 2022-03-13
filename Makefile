all: client server

client: 
	@$(MAKE) -C client

server:
	@$(MAKE) -C server

.PHONY: client server

clean:
	@$(MAKE) clean -C client
	@$(MAKE) clean -C server