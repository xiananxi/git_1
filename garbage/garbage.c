#include <Python.h>
#include "garbage.h"

void garbage_init(){
	Py_Initialize();
	PyObject *sys = PyImport_ImportModule("sys");
	//获取sys.path对象
	PyObject *path = PyObject_GetAttrString(sys,"path");
	//将当前路径.添加到sys.path中，以便加载当前的Python模块（python文件即python模块）
	PyList_Append(path,PyUnicode_FromString("."));   

}

void garbage_final(void){
	Py_Finalize();
}

char *garbage_category(char *category){

	//导入garbage模块，并获取它的引用
	PyObject *pModule = PyImport_ImportModule("garbage");
	//返回值为空，打印错误类型
	if(!pModule){
		PyErr_Print();
		printf("Error:failed to load garbage.py\n");
		goto FAILED_MODULE;
	}
	//获取python函数对象
	PyObject *pFunc = PyObject_GetAttrString(pModule,"alibabacloud_garbage"); 
	if(!pFunc){
        PyErr_Print();
        printf("Error:failed to load alibabacloud_garbage\n");
		goto FAILED_FUNC;
    }
	//调用python函数的参数，获取返回值    函数及传参为空
	PyObject *pValue = PyObject_CallObject(pFunc,NULL);
	if(!pValue){
        PyErr_Print();
        printf("Error:function call failed\n");
		goto FAILED_VALUE;
    }
	char *result = NULL;
	PyArg_Parse(pValue,"s",&result);
	if(!PyArg_Parse){
		PyErr_Print();
		printf("Error:parse failed\n");
		goto FAILED_RESULT;
	}
	printf("return = %s\n",result);
	category = (char *)malloc(sizeof(char)*(strlen(result)+1));
	memset(category,0,(strlen(result)+1));
	strncpy(category,result,(strlen(result)+1));

	//释放所有引用的python对象
FAILED_RESULT:
	Py_DECREF(pValue);
FAILED_VALUE:
	Py_DECREF(pFunc);
FAILED_FUNC:
	Py_DECREF(pModule);
FAILED_MODULE:
	
	return category;
}
