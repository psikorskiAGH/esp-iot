import 'package:aggregator_connector/device/config_field.dart';
import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';

import '../http_path.dart';



class DeviceConfigContext extends ChangeNotifier {
  final HttpJsonApi api;
  int _posting = 0;
  int _reloading = 0;
  List<Widget> widgets = [const Text("Initializing...")];

  DeviceConfigContext({required this.api});

  void post({required String name, String? unit, required dynamic value}) async {
    print("Posting '$name': $value [$unit]");
    if (_posting > 0) {
      print("WARN: DeviceConfig already posting.");
      return;
    }
    ++_posting;
    widgets.clear();
    widgets.add(const Text("Loading..."));
    notifyListeners();

    var data = {
      "name": name,
      "value": value
    };
    if (unit != null) {
      data["unit"] = unit;
    }
    try {
    await api.post([data]);
    } catch (e) {
      widgets.clear();
      widgets.add(Text("Error: ${e.toString()}"));
      --_posting;
      return;
    }
    await reload();
    --_posting;
  }

  Future<void> reload() async {
    print("Updating");
    if (_reloading > 0) {
      print("WARN: DeviceConfig already reloading.");
      return;
    }

    ++_reloading;

    HttpJsonResponse resp;
    try {
      resp = await api.get();
    } catch (e) {
      widgets.clear();
      widgets.add(ConfigDataErrorRow(description: "Error: $e"));
      notifyListeners();
      --_reloading;
      return;
    }
    widgets.clear();
    final respData = await resp.asList();
    for (var el in respData) {
      switch (el["type"]) {
        case "enum":
          widgets.add(ConfigDataEnumRow(data: el, ctx: this));
          break;
        case "integer":
        case "float":
          widgets.add(ConfigDataNumberRow(data: el, ctx: this,));
          break;
        default:
      }
    }
    notifyListeners();
    --_reloading;
  }
}

class DeviceConfig extends StatelessWidget {
  final DeviceConfigContext ctx;

  const DeviceConfig({required this.ctx, super.key});

  @override
  Widget build(BuildContext context) {
    final ret = Padding(
        padding: const EdgeInsets.all(16.0),
        child: ChangeNotifierProvider<DeviceConfigContext>(
          create: (_) => ctx,
          child: Consumer<DeviceConfigContext>(builder: tableBuilder),
        ));
    ctx.reload();
    return ret;
  }

  Widget tableBuilder(
      BuildContext context, DeviceConfigContext ctx, Widget? child) {
    return Column(
      children: ctx.widgets,
    );
  }
}
