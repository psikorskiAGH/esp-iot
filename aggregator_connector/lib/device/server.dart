import 'dart:async';

import 'package:aggregator_connector/device/device.dart';
import 'package:flutter/material.dart';
import 'package:aggregator_connector/http_path.dart';

class DeviceServer extends StatefulWidget {
  final JsonApiHandlers apiHandlers;

  const DeviceServer({super.key, required this.apiHandlers});

  @override
  State<StatefulWidget> createState() {
    return _DeviceServer();
  }
}

class _DeviceServer extends State<DeviceServer> {
  final List<Widget> children = [];
  Future<void>? _loading;

  @override
  void initState() {
    super.initState();
    children.clear();
    children.add(const Text("Loading..."));
    _loading = load();
  }

  Future<void> load() async {
    try {

      setState(() {});

      final HttpJsonResponse resp =
          await widget.apiHandlers.get(subpath: '/devices');

      final data = resp.asList();

      setState(() {
        children.clear();
        children.addAll(
          data.map(
            (name) => Device(
              rootApiHandlers: widget.apiHandlers,
              deviceName: name.toString(),
            ),
          ),
        );
        if (children.isEmpty) {
          children.add(const Text("No devices"));
        }
      });

    } catch (e) {

      if (mounted) {
        setState(() {
          children.clear();
          children.add(Text("Error: $e"));
        });
      }
    }
    _loading = null;
  }

  @override
  void dispose() {
    _loading?.ignore();
    super.dispose();
  }

  _DeviceServer() {
    // Timer(Duration(milliseconds: 1), load);
  }

  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: children.length,
      itemBuilder: (context, index) => children[index],
    );
  }
  //   return ListView(
  //     scrollDirection: Axis.vertical,
  //     shrinkWrap: true,
  //     children: children,
  //   );
  // }
}
