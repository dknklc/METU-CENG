import React, { useEffect , useState} from 'react';
import{Form,Row,Col,Button , Table} from 'react-bootstrap';
import '../pages/addbook.scss';
import HeroSlide from '../components/hero-slide/HeroSlide';
import * as Realm from 'realm-web';
import { BSON } from 'realm-web';

const AddUser = () => {

    const [users,setUsers] = useState([]);

    const getUsers = async () => {
      const app = new Realm.App({ id: "reactapplication-rjdwu" });
      const credentials = Realm.Credentials.anonymous();
      try {
        const user = await app.logIn(credentials);
        const results = await user.functions.getAllUsers();
        setUsers(results);
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
        const result = await user.functions.addUser(e.target.username.value,0,null,0,null);
        console.log(result);
    }
    catch(err) {
        console.error("Failed to log in", err);
    }
  }

  const delUser = async (userid) => {
      const app = new Realm.App({ id: "reactapplication-rjdwu" });
      const credentials = Realm.Credentials.anonymous();
      try {
        const user = await app.logIn(credentials);
        const result = await user.functions.deleteUser(new BSON.ObjectID(userid).toString());
        console.log(result);
      }
      catch(err) {
        console.error("Failed to log in", err);
      }
  }

  useEffect(() => {
    getUsers();
  },[users]);

  return (
    <div>
        <HeroSlide />
        <Form onSubmit={handleSubmit} >
            <Row className="mb-3">
                <Form.Group as={Col} controlId="formGridUserName">
                    <Form.Label>User Name</Form.Label>
                    <Form.Control name="username" placeholder="Enter User Name" />
                </Form.Group>
            </Row>
            <Button variant="primary" type="submit">
                Add
            </Button>
        </Form>

        <Table size="sm" striped bordered hover variant="dark">
            <thead>
                <tr>
                    <th>User Name</th>
                    <th>Number of Books Read</th>
                    <th>Favorite Books</th>
                    <th>Average Rating</th>
                </tr>
            </thead>
            <tbody>
                {
                    users.map(item => 
                        <tr key={item._id}>
                            <td>{item.username}</td>
                            <td>{item.numberOfBooksRead}</td>
                            <td>{item.favoriteBooks}</td>
                            <td>{item.averageRating}</td>
                            <td>
                                <Button variant="danger" onClick={() => delUser(item._id)}>
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

export default AddUser;