public class SeqCheckerTest {
	
	public static void main(String[] args) {
		String dressUpRutine = "generate sequences of {Cap, Jeans, LeftSock, RightSock, Sweater, Tshirt, UnderWear} where {UnderWear < Jeans, Tshirt < Sweater , Sweater < Cap } limit 5;";

		String result = SeqChecker.getSolution(dressUpRutine);

		System.out.printf("Solver version: %d\n", SeqChecker.getVersion());

		System.out.println("Here are 5 ways to dress up:");
		System.out.println(result);
	}
}
