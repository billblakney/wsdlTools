######################################################################
# Automatically generated by qmake (2.01a) Fri Feb 13 13:59:57 2015
######################################################################

TEMPLATE = app
TARGET = app_iec_wsdlFilter
DESTDIR      = $(BIN_DIR)
QMAKE_CLEAN  = $(BIN_DIR)/app_iec_wsdlFilter
QMAKE_CLEAN  += debug/* release/*
CONFIG       += debug_and_release
RESOURCES     = ../../icons/iWsdlToolsIcons.qrc
DEPENDPATH += .
QT += xml

INCLUDEPATH += \
               $(PROTOBUF_DIR)/include \
               $(CCL_LOGGER_INC) \
               $(LOG4CXX_INC) \
               $(BOOST_INC) \
               ../../parser/src \

INCLUDEPATH += .
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

DEFINES += USE_LOG4CXX

include( $(AXB_PROJECT_PRI) )

#LIBS += -L$${LIB_DIR} \
LIBS += \
        -L$(LIB_DIR) -lIecCommonLogger \
        -L$(BOOST_LIB) -lboost_program_options -lboost_regex \
        -L$(LOG4CXX_LIB) \
        -lIecHeaderParser \

# Input
HEADERS += \
           AppConfig.hh \
           AppConfigReader.hh \
           ComboBoxDelegate.hh \
           DataStructModel.hh \
           DelimitActionGroup.hh \
           FieldItem.hh \
           FieldItemData.hh \
           FieldSpec.hh \
           FilterReader.hh \
           FilterSpec.hh \
           HeaderUtil.hh \
           FormatActionGroup.hh \
           LineMatcher.hh \
           MainWindow.hh \
           MessageSpec.hh \
           OperateActionGroup.hh \
           PickFilterDialog.hh \
           RecordLine.hh \
           RecordProcessor.hh \
           RecordWriter.hh \
           SimpleRecordWriter.hh \
           SimpleLineMatcher.hh \
           StreamReader.hh \
           StructTreeView.hh \
           TestRecordBuilder.hh \
           TestRegexDelegate.hh \

SOURCES += \
           main.cc \
           AppConfig.cc \
           AppConfigReader.cc \
           ComboBoxDelegate.cc \
           DataStructModel.cc \
           DelimitActionGroup.cc \
           FieldItem.cc \
           FieldItemData.cc \
           FieldSpec.cc \
           FilterReader.cc \
           FilterSpec.cc \
           HeaderUtil.cc \
           FormatActionGroup.cc \
           LineMatcher.cc \
           MainWindow.cc \
           MessageSpec.cc \
           OperateActionGroup.cc \
           PickFilterDialog.cc \
           RecordLine.cc \
           RecordProcessor.cc \
           RecordWriter.cc \
           SimpleRecordWriter.cc \
           SimpleLineMatcher.cc \
           StreamReader.cc \
           StructTreeView.cc \
           TestRecordBuilder.cc \
           TestRegexDelegate.cc \

