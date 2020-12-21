FROM python:3.7-alpine

COPY requirements.txt /
RUN pip install -r /requirements.txt

RUN mkdir /app
COPY app.py /app
WORKDIR /app

CMD ["python3","app.py"]
