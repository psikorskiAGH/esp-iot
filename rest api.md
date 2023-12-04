# REST API

## Aggregator configuration

`GET /configuration`

`POST /configuration`

<!-- `GET /users`
`GET /users/{user_name}`
`POST /users/{user_name}` -->

## Devices

### Devices list

`GET /devices`

```json
[
    {
        "id": "<string>",
        "name": "<string>",
        "model": "<string>",
    }
]
```

### Device information

`GET /devices/{device_id}`

```json
{
    "id": "<string>",
    "name": "<string>",
    "model": "<string>",
}
```

### Data from device

`GET /devices/{device_id}/data`

Defaults to newest available data

```json
[
    {  // Single data field
        "name": "<string>",
        "unit": "<string, eg. 'km'>",
        "timestamps": ["<int - miliseconds from epoch in UTC>"],
        "data": ["<specified by device>"],
        "Can be extended by device"
    }
]
```

### Data history

`GET /devices/{device_id}/history`

```json
{
    // Time of data collection, fields may extend over this time range
    "from": "<int - miliseconds from epoch in UTC>",
    "to": "<int - miliseconds from epoch in UTC>",
    "data": [
        {
            "timestamp": "<int - miliseconds from epoch in UTC>",
            "fields": [
                // like in 'GET /devices/{device_id}/data'
            ]
        }
    ]
}
```

### Device configuration

`GET /devices/{device_id}/configuration`

Fields declaration:
```json
[
    {
        "name": "<string>",
        "unit": "<string, eg. 'km'>",
        "value": "<specified by device>",
        "Can be extended by device"
    }
]
```

`POST /devices/{device_id}/configuration`

Replaces values in field:
```json
{
    "<name>": {
        "<key>": "<value>",
        "<key>": "<value>",
    }
}
```