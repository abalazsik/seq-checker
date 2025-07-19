#include "SeqChecker.h"
#include "seq-checker-binding.h"
#include <stdbool.h>

JNIEXPORT jstring JNICALL Java_SeqChecker_getSolution(JNIEnv *env, jclass clz, jstring script) {

	jboolean copy = 1;

	const char* scriptAsCharArray = (*env)->GetStringUTFChars(env, script, &copy);

	psolution solution = getSolution(scriptAsCharArray);

	if (isError(solution)) {
		(*env)->ThrowNew(
			env,
			(*env)->FindClass(env,"java.lang.IllegalArgumentException"),
			getSolutionText(solution)
		);
		return NULL;
	}

	return (*env)->NewStringUTF(env, getSolutionText(solution)); 
}

JNIEXPORT jint JNICALL Java_SeqChecker_getVersion(JNIEnv *env, jclass clz) {
	return getVersion();
}