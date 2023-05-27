import mime


def run() -> None:
    doc_view = mime.open("main.go")
    nav_view = doc_view.clone()

    while nav_view.find("func "):
        nav_view.set_mark()
        if not nav_view.find("("):
            raise RuntimeError("Can't find an open paranthesis after func keyword")

        nav_view.backward(1)
        fnname = nav_view.copy()

        doc_view.paste(f"// Python: {fnname};\n")

    doc_view.paste("\n")
    print(doc_view.get_contents())
    doc_view.save_as("mainout.go")


run()
