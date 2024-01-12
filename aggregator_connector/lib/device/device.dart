import 'package:aggregator_connector/device/info.dart';
import 'package:aggregator_connector/http_path.dart';
import 'package:flutter/material.dart';

import 'config.dart';
import 'data.dart';

const Duration dataRefreshInterval = Duration(milliseconds: 1000);

class Device extends StatelessWidget {
  final JsonApiHandlers rootApiHandlers;
  final String deviceName;

  late final DeviceDataContext _deviceDataContext;
  late final DeviceInfoContext _deviceInfoContext;
  late final DeviceConfigContext _deviceConfigContext;

  Device({required this.rootApiHandlers, required this.deviceName, super.key}) {
    _deviceDataContext = DeviceDataContext(
      api: JsonApi(
          handlers: rootApiHandlers, subpath: "/devices/$deviceName/data"),
    );

    _deviceInfoContext = DeviceInfoContext(
      api: JsonApi(handlers: rootApiHandlers, subpath: "/devices/$deviceName"),
    );
    _deviceInfoContext.update();

    _deviceConfigContext = DeviceConfigContext(
      api: JsonApi(
          handlers: rootApiHandlers, subpath: "/devices/$deviceName/config"),
    );
    _deviceConfigContext.reload();

    // broad = _buildBroad();
    // narrow = _buildNarrow();
  }

  @override
  Widget build(BuildContext context) {
    Widget? mode;
    return Column(
      children: [
        LayoutBuilder(
          builder: (context, constraints) {
            if (constraints.maxWidth > 800) {
              // Changing is problematic; just reuse first build created
              return mode ?? (mode = _buildBroad());
            } else {
              return mode ?? (mode = _buildNarrow());
            }
          },
        ),
        Container(
          decoration: const BoxDecoration(
            border: BorderDirectional(
              bottom: BorderSide(
                color: Color(0xFF888888),
              ),
            ),
          ),
        ),
      ],
    );
  }

  Widget _buildBroad() {
    return Row(
      children: [
        Expanded(
          flex: 1,
          child: Column(
            children: [
              _buildDeviceInfo(),
              _buildDeviceConfig(),
            ],
          ),
        ),
        Expanded(
          flex: 1,
          child: _buildDeviceData(),
        ),
      ],
    );
  }

  Widget _buildNarrow() {
    return Column(
      children: [
        _buildDeviceInfo(),
        _buildDeviceConfig(),
        _buildDeviceData(),
      ],
    );
  }

  Widget _buildDeviceData() {
    return DeviceData(
      key: Key('$deviceName/data'),
      ctx: _deviceDataContext,
    );
  }

  Widget _buildDeviceInfo() {
    return DeviceInfo(
      key: Key('$deviceName/info'),
      ctx: _deviceInfoContext,
    );
  }

  Widget _buildDeviceConfig() {
    return DeviceConfig(
      key: Key('$deviceName/config'),
      ctx: _deviceConfigContext,
    );
  }
}
