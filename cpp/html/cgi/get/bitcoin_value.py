import requests

def get_bitcoin_value():
    try:
        url = 'https://api.coincap.io/v2/rates/bitcoin'
        response = requests.get(url)

        if response.status_code == 200:
            data = response.json()
            value = data['data']['rateUsd']
            return ("{\"result\": \"U$ " + value + "\"}")
        else:
            return "{\"Error\": \"Server error\"}"
    except:
        return "{\"Error\": \"Server error\"}"

print(get_bitcoin_value())
