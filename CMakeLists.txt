cmake_minimum_required(VERSION 3.26)

#пробую убрать консольное окно
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")


set(PROJECT_NAME testQTServer)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#здесь мы включаем обязательные опции для QT, fail.ui
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# необходимые компоненты Qt
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

# путь к MySQL
set(MYSQL_INCLUDE_DIR "c:/Program Files/MySQL/MySQL Server 8.0/include")
set(MYSQL_LIB_DIR "c:/Program Files/MySQL/MySQL Server 8.0/lib")

# заголовочные файлы MySQL
include_directories(${MYSQL_INCLUDE_DIR})

#здесь мы включаем обязательные опции для формы
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# исполняемыe файлы
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/mySQLTest.h
    src/mySQLTest.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/mainwindow.ui
    src/ChatServer.h
    src/ChatServer.cpp
    src/QPlainTextBrowserLogger.h
    src/userbandialog.h src/userbandialog.cpp src/userbandialog.ui

)

# Укажите директорию для библиотек
link_directories(${MYSQL_LIB_DIR})

# библиотеки и MySQL
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    ${MYSQL_LIB_DIR}/libmysql.lib # Путь к конкретной библиотеке
)

# Опционально, если необходимо, добавьте следующие строки
# set(MYSQL_LIB "${MYSQL_LIB_DIR}/libmysql.dll")

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "${MYSQL_LIB}" $<TARGET_FILE_DIR:${PROJECT_NAME}>
# )



