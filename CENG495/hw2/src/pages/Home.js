import React from 'react';
import { Link } from 'react-router-dom';
import { OutlineButton } from '../components/button/Button';
import HeroSlide from '../components/hero-slide/HeroSlide';
import BookList from '../components/booklist/BookList';
import { category} from '../api/tmdbApi';


const Home = () => {

  return (
    <div>
      <HeroSlide />
        <div className="container">
          <div className="section mb-3">
            <div className="section__header mb-2">
              <h2>Trending Books</h2>
              <Link to="/books">
                <OutlineButton className="small"> View more </OutlineButton>
              </Link>
            </div>
            <BookList category={category.books} />
          </div>
        </div>
    </div>
  )
}
export default Home;