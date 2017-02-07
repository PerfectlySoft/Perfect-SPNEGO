import PackageDescription

let package = Package(
    name: "PerfectSPNEGO",
    targets: [
      Target(name: "cspnego", dependencies: []),
      Target(name: "PerfectSPNEGO", dependencies: ["cspnego"])
    ]
  )
