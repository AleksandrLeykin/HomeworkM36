cmake_minimum_required(VERSION 3.26)

#пробую убрать консольное окно
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")


set(PROJECT_NAME testQTclient)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#здесь мы включаем обязательные опции для QT, fail.ui
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# необходимые компоненты Qt
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)



#здесь мы включаем обязательные опции для формы
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# исполняемыe файлы
add_executable(${PROJECT_NAME}
    src/main.cpp
	src/sha256.h src/sha256.cpp
        src/mainwindow.h src/mainwindow.cpp src/mainwindow.ui
        src/startscreeen.h src/startscreeen.cpp src/startscreeen.ui
        src/loginform.h src/loginform.cpp src/loginform.ui
        src/registrationform.h src/registrationform.cpp src/registrationform.ui
        src/chatclient.h src/chatclient.cpp
        src/User.h
        src/usernameregisted.h src/usernameregisted.cpp

)

# библиотеки
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Network  
)
