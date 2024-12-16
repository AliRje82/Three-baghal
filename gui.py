import customtkinter as ctk
import subprocess
import os

app = ctk.CTk()
app.geometry("400x500")
app.title("Shopping List Input")

def submit_form():
    user_id = user_id_entry.get()
    shopping_list = shopping_list_entry.get("1.0", "end-1c")  # Get multiline text
    threshold = threshold_entry.get()

    threshold = threshold if threshold else "-1"

    print(f"User ID: {user_id}")
    print(f"Shopping List: {shopping_list}")
    print(f"Threshold: {threshold}")

    # Process the shopping list
    shopping_items = shopping_list.strip().split("\n")  # Split into lines
    number_of_items = len([item for item in shopping_items if item.strip()])  # Count non-empty lines
    shopping_list = ",".join(shopping_items)  # Combine items with commas

    # Clear the inputs
    user_id_entry.delete(0, ctk.END)
    shopping_list_entry.delete("1.0", ctk.END)
    threshold_entry.delete(0, ctk.END)

    try:
        script_path = os.path.abspath("./main")
        subprocess.Popen([
            "gnome-terminal", "--", "zsh", "-c", 
            f"{script_path} {user_id} {shopping_list} {threshold} {str(number_of_items)}; exec zsh"
        ])
    except FileNotFoundError:
        print("Error: Terminal emulator or C program not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

ctk.CTkLabel(app, text="Enter User ID:").pack(pady=10)
user_id_entry = ctk.CTkEntry(app, width=300)
user_id_entry.pack(pady=5)

ctk.CTkLabel(app, text="Enter Shopping List (one item per line) Like Jeans,3:").pack(pady=10)
shopping_list_entry = ctk.CTkTextbox(app, width=300, height=150)
shopping_list_entry.pack(pady=5)

ctk.CTkLabel(app, text="Enter Threshold:").pack(pady=10)
threshold_entry = ctk.CTkEntry(app, width=300)
threshold_entry.pack(pady=5)

submit_button = ctk.CTkButton(app, text="Submit", command=submit_form)
submit_button.pack(pady=20)

app.mainloop()
