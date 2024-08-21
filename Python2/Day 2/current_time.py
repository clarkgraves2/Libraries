from datetime import datetime

def print_current_datetime():
    current = datetime.now()
    formatted_current = current.strftime("%Y-%m-%d %H:%M:%S")
    print(f"Current Date and Time: {current}")
    birthday = datetime(input())
    daystill = current - birthday
    print(f"Days till Birthday: {daystill} ")
if __name__ == "__main__":
    print_current_datetime()
