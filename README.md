# WPF to Native gRPC Calculator

C++ protoc tool takes the proto file and create the needed C++ source and header files
For the sake of the demo, we run them only once aftre each change.
https://github.com/grpc/grpc/blob/v1.34.0/src/cpp/README.md#make
https://github.com/grpc/grpc/blob/v1.34.0/BUILDING.md

We use the vcpkg installation: https://github.com/Microsoft/vcpkg

>vcpkg install grpc:x64-windows-static
>vcpkg install grpc:x86-windows-static

Generating the proto cpp files:
For messages: 
C:\Dev\vcpkg\packages\protobuf_x64-windows-static\tools\protobuf\protoc.exe --cpp_out=. calculator.proto

For server:
C:\Dev\vcpkg\packages\protobuf_x64-windows-static\tools\protobuf\protoc.exe --grpc_out=. --plugin=protoc-gen-grpc="C:\Dev\vcpkg\packages\grpc_x64-windows-static\tools\grpc\grpc_cpp_plugin.exe" calculator.proto

Make sure the cxproj has the correct triplet:
 <PropertyGroup Label="Globals">
   ...
    <VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static</VcpkgTriplet>
    <VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static</VcpkgTriplet>
  </PropertyGroup>
