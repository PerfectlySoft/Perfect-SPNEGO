import XCTest
@testable import PerfectSPNEGO
import cspnego

class PerfectSPNEGOTests: XCTestCase {
    func testExample() {
      #if os(Linux)
        let svc = "HTTP@nut.krb5.ca"
      #else
        let svc = "HTTP@apple.krb5.ca"
      #endif
      Spnego.debug = true
      var token = TOKEN()
      let ret = klogin(svc, &token, 1)
      guard ret == 0 else {
        XCTFail("can not simulate user")
        return
      }//end guard
      print("user logged.")
      do {
        let enc = try Base64.encode(token: token)
        print(enc)
        kfree(token)
        print("try spnego")
        let sp = try Spnego(svc)
        let (usr, token) = try sp.accept(base64Token: enc)
        print("user \(usr) is pending")
        guard let reply = token else {
          print("authentication completed, no further responses required")
          return
        }//end guard
        print("server replied: \(reply)")
      }catch (let err) {
        XCTFail("Error: \(err)")
      }


    }


    static var allTests : [(String, (PerfectSPNEGOTests) -> () throws -> Void)] {
        return [
            ("testExample", testExample),
        ]
    }
}
