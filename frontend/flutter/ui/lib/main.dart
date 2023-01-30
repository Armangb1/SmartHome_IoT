import 'package:flutter/material.dart';
import 'login/login_screen.dart';

// real app
void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(
    const MaterialApp(
      debugShowCheckedModeBanner: false,
      title: "Login Screen ",
      home: LoginScreen(),
    ),
  );
}

// home test
// import 'package:ui/home/home.dart';

// void main(List<String> args) {
//   runApp(const Home());
// }
