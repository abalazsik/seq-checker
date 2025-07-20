#include "SeqChecker.h"
#include "seq-checker-binding.h"
#include <stdbool.h>

JNIEXPORT jstring JNICALL Java_SeqChecker_getSolution(JNIEnv *env, jclass clz, jstring script) {

	jboolean copy = 1;

	const char* scriptAsCharArray = (*env)->GetStringUTFChars(env, script, &copy);

	presult result = getSolution(scriptAsCharArray);

	if (isError(result)) {
		(*env)->ThrowNew(
			env,
			(*env)->FindClass(env,"java.lang.IllegalArgumentException"),
			getResultText(result)
		);
		return NULL;
	}

	return (*env)->NewStringUTF(env, getResultText(result)); 
}

JNIEXPORT jint JNICALL Java_SeqChecker_getVersion(JNIEnv *env, jclass clz) {
	return getVersion();
}