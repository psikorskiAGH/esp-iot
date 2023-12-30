import 'package:flutter/material.dart';

class DeviceDataErrorRow extends StatelessWidget {

  final String description;
  const DeviceDataErrorRow({LocalKey? key, required this.description})
      : super(key: key);
  
  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          child: Text("Error: $description"),
        ),
      ],
    );
  }
}

class DeviceDataValueRow extends StatelessWidget {
  final Map<String, dynamic> data;
  const DeviceDataValueRow({LocalKey? key, required this.data})
      : super(key: key);
  
  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          flex: 1,
          child: Text(data["name"]),
        ),
        Expanded(
          flex: 3,
          child: Text("${data["value"]} [${data["unit"]}]"),
        ),
      ],
    );
  }
}

// class _DeviceDataFieldState extends State<DeviceDataField> {

// }

// class DeviceDataTimearrayField {
//   List<double> values;
//   List<int> timestamps
// }