option(INSTALL_AisinoRand_HEADERS "Install AisinoRand headers." ON)

if(INSTALL_AisinoRand_HEADERS)

    install(FILES AisinoRand/rand.h DESTINATION include/AisinoRand/)

endif(INSTALL_AisinoRand_HEADERS)
