import React , { useState , useEffect } from 'react';
import './bookgrid.scss';
import BookCard from '../bookcard/BookCard'
import * as Realm from 'realm-web';

const BookGrid = props => {

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


  useEffect(() => {
    getBooks();
  }, []);

  return (
      <div className='movie-grid'>
        {
          items.map((item,i) => <BookCard category={props.category} item={item} key={i} />)
        }
      </div>
  )
}

export default BookGrid;