import React from 'react';
import { Route , Switch } from 'react-router-dom';
import Home from '../pages/Home';
import Catalog from '../pages/Catalog';
import Detail from '../pages/detail/Detail';
import AddBook from '../pages/AddBook';
import AddUser from '../pages/AddUser';

const Routes = () => {
  return (
        <Switch>
            <Route path = "/addbook" component={AddBook} />
            <Route path = "/:category/addbook" component={AddBook} />
            <Route path = "/adduser" component={AddUser} />
            <Route path = "/:category/adduser" component={AddUser} />
            <Route path = "/:category/:id" component={Detail} />
            <Route path = "/:category" component={Catalog} />
            <Route path = "/" exact component={Home} />
        </Switch>
  )
}

export default Routes;