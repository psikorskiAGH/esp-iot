import 'dart:math';
import 'dart:async';

import 'package:flutter/material.dart';

import 'device/device.dart';

Random random = Random();
Device osc = const Device();

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
          children: const [
            Device(),
            // Text("Hi")
          ],
        ),
      ),
    );
  }
}
