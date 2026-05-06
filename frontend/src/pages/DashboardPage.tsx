import React, { useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { useAuth } from '../context/AuthContext';
import { Loader } from 'lucide-react';

const DashboardPage: React.FC = () => {
  const { user, isAuthenticated, isLoading } = useAuth();
  const navigate = useNavigate();

  useEffect(() => {
    if (!isLoading) {
      if (!isAuthenticated || !user) {
        navigate('/login', { replace: true });
        return;
      }

      // Redirect to role-specific dashboard
      const role = user.role.toUpperCase();
      switch (role) {
        case 'AUTHOR':
          navigate('/author/dashboard', { replace: true });
          break;
        case 'REVIEWER':
          navigate('/reviewer/dashboard', { replace: true });
          break;
        case 'ORGANIZER':
          navigate('/organizer/dashboard', { replace: true });
          break;
        case 'ADMIN':
          navigate('/admin/dashboard', { replace: true });
          break;
        default:
          navigate('/author/dashboard', { replace: true }); // Fallback
      }
    }
  }, [user, isAuthenticated, isLoading, navigate]);

  return (
    <div className="flex justify-center items-center h-64">
      <Loader className="w-8 h-8 text-blue-500 animate-spin" />
    </div>
  );
};

export default DashboardPage;
