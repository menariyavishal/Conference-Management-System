import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { AuthProvider } from './context/AuthContext';

import Navbar from './components/layout/Navbar';
import Hero from './components/layout/Hero';
import ConferenceSlider from './components/layout/ConferenceSlider';
import AuthPage from './pages/AuthPage';

// Dashboard Components
import ProtectedRoute from './components/auth/ProtectedRoute';
import DashboardLayout from './components/layout/DashboardLayout';
import DashboardPage from './pages/DashboardPage';
import AuthorDashboard from './components/dashboard/AuthorDashboard';
import ReviewerDashboard from './components/dashboard/ReviewerDashboard';
import OrganizerDashboard from './components/dashboard/OrganizerDashboard';

const LandingPage: React.FC = () => {
  return (
    <div className="bg-[#0a0f1e] text-white min-h-screen">
      <Navbar />
      <Hero />
      <ConferenceSlider />
      
      <footer className="relative z-10 text-center py-10 text-slate-500 text-sm border-t border-white/10">
        <p>© 2026 Conference Manager – Built with C++17 & React | All rights reserved</p>
      </footer>
    </div>
  );
};

import { Toaster } from 'react-hot-toast';

function App() {
  return (
    <AuthProvider>
      <Router>
        <Routes>
          {/* Public Routes */}
          <Route path="/" element={<LandingPage />} />
          <Route path="/login" element={<AuthPage />} />
          <Route path="/register" element={<AuthPage />} />
          
          {/* Protected Dashboard Routes */}
          <Route element={<ProtectedRoute />}>
            <Route element={<DashboardLayout />}>
              <Route path="/dashboard" element={<DashboardPage />} />
              
              <Route path="/author/dashboard" element={
                <ProtectedRoute allowedRoles={['AUTHOR']}><AuthorDashboard /></ProtectedRoute>
              } />
              <Route path="/reviewer/dashboard" element={
                <ProtectedRoute allowedRoles={['REVIEWER']}><ReviewerDashboard /></ProtectedRoute>
              } />
              <Route path="/organizer/dashboard" element={
                <ProtectedRoute allowedRoles={['ORGANIZER', 'ADMIN']}><OrganizerDashboard /></ProtectedRoute>
              } />
              <Route path="/admin/dashboard" element={
                <ProtectedRoute allowedRoles={['ORGANIZER', 'ADMIN']}><OrganizerDashboard /></ProtectedRoute>
              } />
            </Route>
          </Route>
        </Routes>
        <Toaster 
          position="bottom-right"
          toastOptions={{
            duration: 4000,
            style: {
              background: '#0f172a',
              color: '#fff',
              border: '1px solid rgba(255,255,255,0.1)',
              backdropFilter: 'blur(12px)',
              borderRadius: '16px',
              padding: '12px 24px',
            },
            success: {
              iconTheme: {
                primary: '#3b82f6',
                secondary: '#fff',
              },
            },
          }}
        />
      </Router>
    </AuthProvider>
  );
}

export default App;
