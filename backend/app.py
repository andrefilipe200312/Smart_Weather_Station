from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS
from influxdb_client import InfluxDBClient
from dotenv import load_dotenv
import os

# Carregar variáveis de ambiente do .env
load_dotenv()

app = Flask(__name__, static_folder="build", static_url_path="/")
CORS(app)

# Configuração do InfluxDB via variáveis de ambiente
INFLUX_URL = "https://us-east-1-1.aws.cloud2.influxdata.com"
INFLUX_TOKEN = "hWyTCLL5vqx0C0cjdIVC99n2Eo1gdrtGesFqeK7nmynfFfsREfH08hCdHjQ6UvIOnqwgX5EPrAj6ghIS-GIrRQ=="
INFLUX_ORG = "Reis do TEmpo"
INFLUX_BUCKET = "estacao_meteo"

client = InfluxDBClient(url=INFLUX_URL, token=INFLUX_TOKEN, org=INFLUX_ORG)
query_api = client.query_api()

@app.route("/api/hello")
def hello():
    return jsonify(message="Olá do Flask!")

@app.route("/api/clima")
def get_clima_data():
    try:
        query = f'''
        from(bucket: "{INFLUX_BUCKET}")
          |> range(start: -7d)
          |> filter(fn: (r) => r._measurement == "clima")
          |> filter(fn: (r) => r._field =~ /^Temperatura$|^Humidade$|^Direcao_vento$|^Velocidade_vento$|^Pluviosidade$|^Bateria$/)
          |> last()
        '''
        result = query_api.query(query=query)
        data = {}
        for table in result:
            for record in table.records:
                field_name = record.get_field()
                value = record.get_value()
                time = record.get_time()
                data[field_name] = {
                    "value": value,
                    "time": time.isoformat() if time else None
                }
        return jsonify(success=True, data=data, message="Dados climáticos obtidos com sucesso")
    except Exception as e:
        return jsonify(success=False, error=str(e), message="Erro ao obter dados climáticos"), 500

@app.route("/api/clima/aggregated")
def get_clima_aggregated():
    try:
        selected_range = request.args.get("range", "-5d")
        query = f'''
        from(bucket: "{INFLUX_BUCKET}")
          |> range(start: {selected_range})
          |> filter(fn: (r) => r["_measurement"] == "clima")
          |> filter(fn: (r) => r["_field"] == "Humidade" or r["_field"] == "Pluviosidade" or r["_field"] == "Temperatura" or r["_field"] == "Velocidade_vento")
          |> aggregateWindow(every: 1m, fn: mean, createEmpty: false)
          |> yield(name: "mean")
        '''
        result = query_api.query(query=query)
        data = {}
        for table in result:
            for record in table.records:
                field = record.get_field()
                if field not in data:
                    data[field] = []
                data[field].append({
                    "time": record.get_time().isoformat() if record.get_time() else None,
                    "value": record.get_value()
                })

        return jsonify(success=True, range=selected_range, data=data, message="Dados agregados obtidos com sucesso")
    except Exception as e:
        return jsonify(success=False, error=str(e), message="Erro ao obter dados agregados"), 500


@app.route("/", defaults={"path": ""})
@app.route("/<path:path>")
def serve_react(path):
    if path != "" and os.path.exists(os.path.join(app.static_folder, path)):
        return send_from_directory(app.static_folder, path)
    else:
        return send_from_directory(app.static_folder, "index.html")

if __name__ == "__main__":
    app.run(debug=False, host="0.0.0.0", port=int(os.environ.get("PORT", 5000)))
