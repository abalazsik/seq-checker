import java.io.File;;


public class SeqChecker {

	static {
		System.load(new File("./seq-checker.so").getAbsolutePath());
	}

	public static native String getSolution(String script);
	public static native int getVersion();
}
