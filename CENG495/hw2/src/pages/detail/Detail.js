import React , { useEffect , useState } from 'react';
import { useParams } from 'react-router-dom';
import './detail.scss';
import * as Realm from 'realm-web';
import { BSON } from 'realm-web';
import Rating from '@mui/material/Rating';
import Typography from '@mui/material/Typography';

const Detail = () => {

  const { category , id } = useParams();
  const [item, setItem] = useState(null);
  //const [value, setValue] = useState();


  const update = async (newValue) => {
    const app = new Realm.App({ id: "reactapplication-rjdwu" });
    const credentials = Realm.Credentials.anonymous();
    try {
      const user = await app.logIn(credentials);
      const results = await user.functions.updateAverageRatingofBook(new BSON.ObjectID(item._id).toString(),(item.averageRatingOftheBook + newValue)/2);
      setItem(results);
    }
    catch(err) {
      console.error("Failed to log in", err);
    }
  }

  useEffect(() => {
    const getSinBook = async () => {
        const app = new Realm.App({ id: "reactapplication-rjdwu" });
        const credentials = Realm.Credentials.anonymous();
        try {
            const user = await app.logIn(credentials);
            const result = await user.functions.getSingleBook(new BSON.ObjectID(id).toString());
            console.log(result);
            setItem(result);
        }
        catch(err) {
            console.error("Failed to log in", err);
        }
      }
    window.scrollTo(0,0);
    getSinBook();
  },[category,id]);

  return (
      
    <>
        {
            item && (
                <>
                    <div className="banner" style={{backgroundImage: `url(${item.wallpaper})`}}></div>
                    <div className="mb-3 movie-content container">
                        <div className="movie-content__poster">
                            <div className="movie-content__poster__img" style={{backgroundImage: `url(${item.img})`}}></div>
                        </div>
                        <div className="movie-content__info">
                            <h1 className="title">
                                {item.bookName || item.author}
                            </h1>
                            <div className="genres">
                                {
                                    item.genres && item.genres.slice(0, 5).map((genre, i) => (
                                        <span key={i} className="genres__item">{genre}</span>
                                    ))
                                }
                            </div>
                            <p className="overview">{item.description}</p>
                            <div className="genres">
                                <p className="overview">Average Rating : {item.averageRatingOftheBook}</p>
                                    <Typography component="legend">Rate the book</Typography>
                                    <Rating
                                        name="simple-controlled"
                                        value={item.averageRatingOftheBook}
                                        onChange={(event, newValue) => {
                                        update(parseInt(newValue));
                                    }}
                                    />
                            </div>
                            
                            <div className="cast">
                                <div>
                                    <h2>Reviews</h2>
                                    <div className="genres">
                                        {
                                            item.reviews.map((review,i) => (
                                                <ul style={{ listStyleType: "circle" }} >
                                                    <li key={i}>{review}</li>
                                                </ul>
                            
                                            ))
                                        }
                                    </div>
                                    
                                </div>
                            </div>
                        </div>
                    </div>
                </>
            )
        }
    </>
);
}

export default Detail;