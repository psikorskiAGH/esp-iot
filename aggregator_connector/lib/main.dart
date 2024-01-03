import 'package:flutter/material.dart';

import 'device/device.dart';

void main() {
  runApp(const MainApp());
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: ListView(
          scrollDirection: Axis.vertical,
          shrinkWrap: true,
          children: [
            Device(),
            // Text("Hi")
          ],
        ),
      ),
    );
  }
}
