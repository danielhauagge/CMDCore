ADD_EXECUTABLE( test_tok test_tok.cpp )
TARGET_LINK_LIBRARIES( test_tok clc )

ADD_EXECUTABLE( test_logger test_logger.cpp )
TARGET_LINK_LIBRARIES( test_logger clc )

ADD_EXECUTABLE( test_optparser test_optparser.cpp )
TARGET_LINK_LIBRARIES( test_optparser clc )
