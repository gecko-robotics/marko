

def test_import():
    try:
        import pymarko
        assert True
    except ImportError:
        assert False
