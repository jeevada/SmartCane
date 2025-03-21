set(srcs "hello_world_main.c"
	     "connect_wifi.c")
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")
