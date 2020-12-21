import flask
from flask import json
import requests  
import json
import os

app = flask.Flask(__name__)
data={}
PROMETHEUS_URL = os.getenv("PROMETHEUS_URL","http://prometheus-kube-prometheus-prometheus.prometheus.svc.cluster.local:9090")
PROMETHEUS = PROMETHEUS_URL+"/api/v1/query"

@app.route('/metrics')
def metrics():
    getMetrics()
    response = app.response_class(
        response=json.dumps(data),
        status=200,
        mimetype='application/json'
    )
    return response

def getMetrics():
    global data

    #Node count
    response = requests.get(PROMETHEUS,
    params={'query': 'count(kube_node_info)'})  
    results = response.json()
    data["nodes"]=results['data']['result'][0]['value'][1]

    #CPU
    response = requests.get(PROMETHEUS,
    params={'query': '(1-avg(rate(node_cpu_seconds_total{mode="idle", cluster=""}[5m])))*100'})
    results = response.json()
    data["cpu"]=str(round(float(results['data']['result'][0]['value'][1]),2))

    #Memory
    response = requests.get(PROMETHEUS,
    params={'query': '(1 - sum(:node_memory_MemAvailable_bytes:sum{cluster=""}) / sum(kube_node_status_allocatable_memory_bytes{cluster=""}))*100'})
    results = response.json()
    data["memory"]=str(round(float(results['data']['result'][0]['value'][1]),2))

    #Pod count
    response = requests.get(PROMETHEUS,
    params={'query': 'sum(kubelet_running_pods{cluster="", job="kubelet", metrics_path="/metrics", instance=~"(10.0.0.70:10250|10.0.0.71:10250|10.0.0.72:10250)"})'})
    results = response.json()
    data["pods"]=results['data']['result'][0]['value'][1]

    #Container count
    response = requests.get(PROMETHEUS,
    params={'query': 'sum(kubelet_running_containers{cluster="", job="kubelet", metrics_path="/metrics", instance=~"(10.0.0.70:10250|10.0.0.71:10250|10.0.0.72:10250)"})'})
    results = response.json()
    data["containers"]=results['data']['result'][0]['value'][1]

    #K8s Build
    response = requests.get(PROMETHEUS,
    params={'query': 'kubernetes_build_info'})
    results = response.json()
    ver=results['data']['result'][0]['metric']['git_version']
    data["version"]=ver

if __name__ == "__main__":
    print("Loading server")       
    app.run(host='0.0.0.0')
