import React, { useEffect , useState} from 'react';
import{Form,Row,Col,Button , Table} from 'react-bootstrap';
import '../pages/addbook.scss';
import HeroSlide from '../components/hero-slide/HeroSlide';
import * as Realm from 'realm-web';
import { BSON } from 'realm-web';

const AddBook = () => {

    const [items,setItems] = useState([]);

    const getBooks = async () => {
      const app = new Realm.App({ id: "reactapplication-rjdwu" });
      const credentials = Realm.Credentials.anonymous();
      try {
        const user = await app.logIn(credentials);
        const results = await user.functions.getAllBooks();
        setItems(results);
      }
      catch(err) {
        console.error("Failed to log in", err);
      }
    }

  const handleSubmit = async (e) => {
    e.preventDefault();
    const app = new Realm.App({ id: "reactapplication-rjdwu" });
    const credentials = Realm.Credentials.anonymous();
    try {
        const user = await app.logIn(credentials);
        const result = await user.functions.addBook(e.target.BookName.value,e.target.Author.value,e.target.Translator.value,e.target.Editor.value,e.target.isFriction.value,e.target.Publisher.value,e.target.YearPublished.value,0,null,'data:image/png;base64,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',null,e.target.Genre.value);
        console.log(result);
    }
    catch(err) {
        console.error("Failed to log in", err);
    }
  }

  const delBook = async (bookid) => {
      const app = new Realm.App({ id: "reactapplication-rjdwu" });
      const credentials = Realm.Credentials.anonymous();
      try {
        const user = await app.logIn(credentials);
        const result = await user.functions.deleteBook(new BSON.ObjectID(bookid).toString());
        console.log(result);
      }
      catch(err) {
        console.error("Failed to log in", err);
      }
  }

  useEffect(() => {
    getBooks();
  },[items]);

  return (
    <div>
        <HeroSlide />
        <Form onSubmit={handleSubmit} >
            <Row className="mb-3">
                <Form.Group as={Col} controlId="formGridBookName">
                    <Form.Label>Book Name</Form.Label>
                    <Form.Control name="BookName" placeholder="Enter Book Name" />
                </Form.Group>

                <Form.Group as={Col} controlId="formGridAuthor">
                    <Form.Label>Author</Form.Label>
                    <Form.Control name="Author" placeholder="Enter Author" />
                </Form.Group>
            </Row>
            <Row className="mb-3">
                <Form.Group as={Col} controlId="formGridTranslator">
                    <Form.Label>Translator</Form.Label>
                    <Form.Control name="Translator" placeholder="Enter translator if any" />
                </Form.Group>

                <Form.Group as={Col} controlId="formGridEditor">
                    <Form.Label>Editor</Form.Label>
                    <Form.Control name="Editor" placeholder="Enter editor if any" />
                </Form.Group>
            </Row>
            <Row className="mb-3">
                <Form.Group as={Col} controlId="formGridYearPublished">
                    <Form.Label>Year Published</Form.Label>
                    <Form.Control name="YearPublished" placeholder="Enter year published" />
                </Form.Group>
            </Row>
            <Row className="mb-3">
                <Form.Group as={Col} controlId="formGridPublisher">
                <Form.Label>Publisher</Form.Label>
                <Form.Control name="Publisher" placeholder="Enter publisher" />
                </Form.Group>

                <Form.Group as={Col} controlId="formGridFriction">
                <Form.Label>isFriction</Form.Label>
                <Form.Select name="isFriction" defaultValue="Choose...">
                    <option>true</option>
                    <option>false</option>
                </Form.Select>
                </Form.Group>

                <Form.Group as={Col} controlId="formGridGenre">
                <Form.Label>Genre</Form.Label>
                <Form.Control name="Genre" placeholder="Enter Genre" />
                </Form.Group>
            </Row>

            <Button variant="primary" type="submit">
                Add
            </Button>
        </Form>

        <Table size="sm" striped bordered hover variant="dark">
            <thead>
                <tr>
                    <th>Book Name</th>
                    <th>Author</th>
                    <th>Option</th>
                </tr>
            </thead>
            <tbody>
                {
                    items.map(item => 
                        <tr key={item._id}>
                            <td>{item.bookName}</td>
                            <td>{item.author}</td>
                            <td>
                                <Button variant="danger" onClick={() => delBook(item._id)}>
                                    Delete
                                </Button>
                            </td>
                        </tr>
                    )
                }
               
                    
            </tbody>
        </Table>
    </div>
  )
}

export default AddBook;